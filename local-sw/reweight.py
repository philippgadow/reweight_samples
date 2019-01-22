#!/usr/bin/env python

"""
Simple script to draw some distributions.
"""

import h5py
import argparse
import numpy as np
import pandas as pd
from matplotlib import pyplot as plt
from hep_ml import reweight
from sklearn.model_selection import train_test_split


def getArgs():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('original')
    parser.add_argument('target')
    parser.add_argument('-o', '--output-dir', default='plots')
    return parser

def drawDistributions(original, target, new_original_weights, columns, outputname='test.png'):
    hist_settings = {'bins': 100, 'density': True, 'alpha': 0.7}
    plt.figure(figsize=[15, 7])
    for id, column in enumerate(columns, 1):
        xlim = np.percentile(np.hstack([target[column]]), [0.01, 99.99])
        plt.subplot(2, 3, id)
        plt.hist(original[column], weights=new_original_weights, range=xlim, **hist_settings)
        plt.hist(target[column], range=xlim, **hist_settings)
        plt.title(column)
    plt.savefig(outputname)

def readData(inputFile):
    with h5py.File(inputFile, 'r') as infile:
        truthparticles = np.asarray(infile['truthparticles']['pt'])
        truthmet = np.asarray(infile['eventvariables']['truthmet']).flatten()
        truthdRWW = np.asarray(infile['eventvariables']['truthdRWW']).flatten()

    df = pd.DataFrame(truthparticles, columns=["zp_pt", "hs_pt", "wp_pt", "wm_pt"])
    df['met'] = pd.Series(truthmet)
    df['dRWW'] = pd.Series(truthdRWW)

    # clean up
    df = df.dropna()
    df.drop(columns=['zp_pt'])

    return df

def run():
    args = getArgs().parse_args()

    original = readData(args.original)
    target = readData(args.target)

    original_weights = np.ones(len(original))
    target_weights = np.ones(len(target))

    # divide original samples into training ant test parts
    original_train, original_test = train_test_split(original)
    # divide target samples into training ant test parts
    target_train, target_test = train_test_split(target)

    original_weights_train = np.ones(len(original_train))
    original_weights_test = np.ones(len(original_test))

    columns = ["hs_pt", "wp_pt", "wm_pt", 'met', 'dRWW']

    print("train", len(original_train))
    print("test", len(original_test))

    # draw full distributions
    drawDistributions(original, target, original_weights, columns, 'total.png')
    # draw train distributions
    drawDistributions(original_train, target_train, original_weights_train, columns, 'train.png')
    # draw test distributions
    drawDistributions(original_test, target_test, original_weights_test, columns, 'test_before.png')

    # # bin based reweighting
    # bins_reweighter = reweight.BinsReweighter(n_bins=20, n_neighs=1.)
    # bins_reweighter.fit(original_train, target_train)
    # bins_weights_test = bins_reweighter.predict_weights(original_test)
    # # validate reweighting rule on the test part comparing 1d projections
    # drawDistributions(original_test, target_test, bins_weights_test, columns, 'test_bins.png')

    # gradient boosted reweighting
    reweighter = reweight.GBReweighter(n_estimators=200, learning_rate=0.05, max_depth=3, min_samples_leaf=1000, 
                                       gb_args={'subsample': 0.4})
    reweighter.fit(original_train, target_train)
    gb_weights_test = reweighter.predict_weights(original_test)
    # validate reweighting rule on the test part comparing 1d projections
    drawDistributions(original_test, target_test, gb_weights_test, columns, 'test_bdt.png')


if __name__ == '__main__':
    run()
