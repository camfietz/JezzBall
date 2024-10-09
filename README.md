# Custom JezzBall in C++ #
SENG475: Advanced Programming Techniques for Robust and Efficient Computing
* Cameron Fietz

## PRESENTATION
https://youtu.be/vdi3znqS1TI

## REQUIREMENTS
`SDL 1.2.15`

## BUILD & INSTALL
Let $TOP_DIR denote the directory containing this README file.

#### To build and install the software, use the commands:
    cd $TOP_DIR
    cmake -H. -Btmp_cmake -DCMAKE_INSTALL_PREFIX=install_dir
    cmake --build tmp_cmake --clean-first --target install

#### To run a demonstration, use the commands:
    install_dir/bin/jezzball
