#!/usr/bin/env python

"""
Simple script to draw some distributions.
"""

import h5py
import argparse
import numpy as np
import pandas as pd
from os import makedirs
from os.path import join
from matplotlib import pyplot as plt
from hep_ml import reweight
from sklearn.model_selection import train_test_split


def getArgs():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('original')
    parser.add_argument('target')
    parser.add_argument('--result', default=None)
    parser.add_argument('-o', '--outputdir', default='plots')
    return parser

def drawDistributions(original, target, new_original_weights, columns, outputname='test.png'):
    hist_settings = {'bins': 100, 'density': True, 'alpha': 0.7}
    plt.figure(figsize=[15, 7])
    for id, column in enumerate(columns, 1):
        xlim = np.percentile(np.hstack([target[column]]), [0.01, 99.99])
        plt.subplot(2, 4, id)
        plt.hist(original[column], weights=new_original_weights, range=xlim, label='original', **hist_settings)
        plt.hist(target[column], range=xlim, label='target', **hist_settings)
        plt.title(column)
        plt.legend(loc='best')
    plt.savefig(outputname)


def readData(inputFile, clean=True):
    with h5py.File(inputFile, 'r') as infile:
        truthparticles_pt = np.asarray(infile['truthparticles']['pt'])
        truthparticles_eta = np.asarray(infile['truthparticles']['eta'])
        truthmet = np.asarray(infile['eventvariables']['truthmet']).flatten()
        truthdRWW = np.asarray(infile['eventvariables']['truthdRWW']).flatten()



    df = pd.DataFrame(truthparticles_pt, columns=['zp_pt', 'hs_pt', 'wp_pt', 'wm_pt'])
    df['met'] = pd.Series(truthmet)
    truthparticles_eta = pd.DataFrame(truthparticles_eta, columns=['zp_eta', 'hs_eta', 'wp_eta', 'wm_eta'])
    df['hs_abseta'] = truthparticles_eta['hs_eta'].abs()
    df['wp_abseta'] = truthparticles_eta['wp_eta'].abs()
    df['wm_abseta'] = truthparticles_eta['wm_eta'].abs()
    df['dRWW'] = pd.Series(truthdRWW)    
    if clean:
        df = df.dropna()
    else:
        df = df.fillna(0.)
    df.drop(columns=['zp_pt'])

    return df

def run():
    args = getArgs().parse_args()

    ##
    ## TRAINING + VALIDATION OF BDT
    ##

    original = readData(args.original)
    target = readData(args.target)

    original_weights = np.ones(len(original))
    target_weights = np.ones(len(target))

    # divide original samples into training and test parts
    original_train, original_test = train_test_split(original)
    # divide target samples into training and test parts
    target_train, target_test = train_test_split(target)

    original_weights_train = np.ones(len(original_train))
    original_weights_test = np.ones(len(original_test))

    columns = ['hs_pt', 'wp_pt', 'wm_pt', 'met', 'hs_abseta', 'wp_abseta', 'wm_abseta', 'dRWW']

    print('train', len(original_train))
    print('test', len(original_test))

    # create output folder
    try: makedirs(args.outputdir)
    except OSError: pass
    # draw full distributions
    drawDistributions(original, target, original_weights, columns, join(args.outputdir, 'total.png'))
    # draw train distributions
    drawDistributions(original_train, target_train, original_weights_train, columns, join(args.outputdir, 'train.png'))
    # draw test distributions
    drawDistributions(original_test, target_test, original_weights_test, columns, join(args.outputdir, 'test_before.png'))

    # gradient boosted reweighting
    reweighter = reweight.GBReweighter(n_estimators=200, learning_rate=0.1, max_depth=4, min_samples_leaf=1000, 
                                       gb_args={'subsample': 0.4})
    reweighter.fit(original_train, target_train)
    gb_weights_test = reweighter.predict_weights(original_test)
    # validate reweighting rule on the test part comparing 1d projections
    drawDistributions(original_test, target_test, gb_weights_test, columns, join(args.outputdir, 'test_bdt.png'))


    ##
    ## REWEIGHTING THE SIMULATED SIGNAL SAMPLE
    ##
    if args.result:
        result = readData(args.result, clean=False)
        result_weights = np.ones(len(result))
        print('result', len(result))
        # reweight result
        gb_weights_result = reweighter.predict_weights(result)

        # plot result comparing 1d projections
        drawDistributions(result, target_test, result_weights, columns, join(args.outputdir, 'result_before.png'))
        drawDistributions(result, target_test, gb_weights_result, columns, join(args.outputdir, 'result_bdt.png'))


if __name__ == '__main__':
    run()
