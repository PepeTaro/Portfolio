import os
from setuptools import setup

directory = os.path.abspath(os.path.dirname(__file__))
with open(os.path.join(directory, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

setup(
    name='quantum',
    version='1.0',
    description='ノイズレス簡易量子コンピューターシミュレーター',
    long_description=long_description,
    author='PepeTaro',
    packages=['quantum'],
    install_requires=['numpy'],
)


