from setuptools import find_packages
from setuptools import setup

setup(
    name='scara_msgs',
    version='0.0.0',
    packages=find_packages(
        include=('scara_msgs', 'scara_msgs.*')),
)
