!/bin/bash

# setup python virtualenv
wget https://files.pythonhosted.org/packages/33/bc/fa0b5347139cd9564f0d44ebd2b147ac97c36b2403943dbee8a25fd74012/virtualenv-16.0.0.tar.gz
tar xvf virtualenv-16.0.0.tar.gz
python2 virtualenv-16.0.0/virtualenv.py venv
rm -r virtualenv-16.0.0  virtualenv-16.0.0.tar.gz

# activate virtual environment
source venv/bin/activate

# install common python packages
pip install pip argparse grin --upgrade
pip install numpy pandas matplotlib scipy tables hep_ml sklearn
