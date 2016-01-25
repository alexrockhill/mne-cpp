# ###startdir### %WORKSPACE%/mne-cpp/..
# %0 Build Number
# arg0=%0

echo Starting MNE-CPP Linux Ubuntu Build

mkdir mne-cpp_shadow_build
cd mne-cpp_shadow_build

QT_BIN_DIR='/opt/Qt5.6.0/5.6/gcc_64/bin'
QT_LIB_DIR='/opt/Qt5.6.0/5.6/gcc_64/lib'

$QT_BIN_DIR/qmake ../mne-cpp/mne-cpp.pro -r
make clean
make -j4

# user package
cd ..
rm mne-cpp-linux-x86_64-1.0.0-beta.tar.gz
tar cfvz mne-cpp-linux-x86_64-1.0.0-beta.tar.gz ./mne-cpp/bin ./mne-cpp/lib

# development package
# tar cfvz mne-cpp-linux-x86_64-1.0.0-beta_dev.tar.gz *
