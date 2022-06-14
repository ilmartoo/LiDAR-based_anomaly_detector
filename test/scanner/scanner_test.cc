/**
 * @file scanner_test.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 10/05/2022
 *
 * Test para el módulo de adquisición de datos
 *
 */

#include "catch.hpp"
#include "catch_utils.hh"

#include "scanner/IScanner.hh"
#include "scanner/IFileScanner.hh"
#include "scanner/ScannerLidar.hh"
#include "scanner/ScannerLVX.hh"
#include "scanner/ScannerCSV.hh"

#include "models/LidarPoint.hh"

/* MOCKUPS */
class ScannerLidarMock : public ScannerLidar {
   public:
    ScannerLidarMock(const char c[kBroadcastCodeSize]) : ScannerLidar(c) {}
};
class ScannerCSVMock : public ScannerCSV {
   public:
    ScannerCSVMock(const std::string &file) : ScannerCSV(file) {}
};
class ScannerLVXMock : public ScannerLVX {
   public:
    ScannerLVXMock(const std::string &file) : ScannerLVX(file) {}
};
/***********/

class CallbackFixture {
   public:
    IScanner *sc;
    ScannerLidarMock slm;
    ScannerCSVMock svm;
    ScannerLVXMock sxm;

    ScannerLidar &sl;
    ScannerCSV &sv;
    ScannerLVX &sx;

    ScanCode scd;

    CallbackFixture() : sc(nullptr),
                        slm("3WEDH7600101621"),
                        svm("../../test/scanner/testdata.csv"),
                        sxm("../../test/scanner/testdata.lvx"),
                        sl(slm),
                        sv(svm),
                        sx(sxm) {}

    void callbackScan(const LidarPoint &p) {
        scd = sc->scan();
        sc->pause();
    }
    void callbackEOF(const LidarPoint &p) {}
    void callbackStop(const LidarPoint &p) { sc->pause(); }
};

TEST_CASE_METHOD(CallbackFixture, "1.1", "[ScannerLidar]") {
    CHECK(sl.init());
}

TEST_CASE_METHOD(CallbackFixture, "1.2", "[ScannerLidar]") {
    sl.init();
    CHECK(!sl.init());
}

// TEST_CASE_METHOD(CallbackFixture, "1.3", "[ScannerLidar]") {
//     sc = &sl;
//     sl.init();
//     sl.setCallback([this](const LidarPoint &p) { this->callbackStop(p); });
//     CHECK(sl.scan() == kScanOk);
// }

// TEST_CASE_METHOD(CallbackFixture, "1.4", "[ScannerLidar]") {
//     sc = &sl;
//     sl.init();
//     sl.setCallback([this](const LidarPoint &p) { this->callbackScan(p); });
//     sl.scan();
//     CHECK(scd == kScanError);
// }

TEST_CASE_METHOD(CallbackFixture, "1.5", "[ScannerLidar]") {
    sl.setCallback([this](const LidarPoint &p) { this->callbackStop(p); });
    sl.scan();
    CHECK(!sl.isScanning());
}

TEST_CASE_METHOD(CallbackFixture, "1.6", "[ScannerLidar]") {
    sl.init();
    sl.stop();
    CHECK(sl.init());
}

TEST_CASE_METHOD(CallbackFixture, "1.7", "[ScannerCSV]") {
    CHECK(sv.init());
}

TEST_CASE_METHOD(CallbackFixture, "1.8", "[ScannerCSV]") {
    ScannerCSVMock sm("");
    CHECK(!sm.init());
}

TEST_CASE_METHOD(CallbackFixture, "1.9", "[ScannerCSV]") {
    ScannerCSVMock sm("");
    CHECK(sm.scan() == kScanError);
}

TEST_CASE_METHOD(CallbackFixture, "1.10", "[ScannerCSV]") {
    sv.scan();
    sv.setCallback([this](const LidarPoint &p) { this->callbackStop(p); });
    CHECK(sv.scan() == kScanOk);
}

TEST_CASE_METHOD(CallbackFixture, "1.11", "[ScannerCSV]") {
    sv.setCallback([this](const LidarPoint &p) { this->callbackScan(p); });
    sv.scan();
    CHECK(scd == kScanError);
}

TEST_CASE_METHOD(CallbackFixture, "1.12", "[ScannerCSV]") {
    sv.setCallback([this](const LidarPoint &p) { this->callbackStop(p); });
    sv.scan();
    CHECK(!sv.isScanning());
}

TEST_CASE_METHOD(CallbackFixture, "1.13", "[ScannerCSV]") {
    sv.init();
    sv.stop();
    CHECK(sv.init());
}

TEST_CASE_METHOD(CallbackFixture, "1.14", "[ScannerLVX]") {
    CHECK(sx.init());
}

TEST_CASE_METHOD(CallbackFixture, "1.15", "[ScannerLVX]") {
    ScannerLVXMock sm("");
    CHECK(!sm.init());
}

TEST_CASE_METHOD(CallbackFixture, "1.16", "[ScannerLVX]") {
    ScannerLVXMock sm("");
    CHECK(sm.scan() == kScanError);
}

TEST_CASE_METHOD(CallbackFixture, "1.17", "[ScannerLVX]") {
    sx.scan();
    sx.setCallback([this](const LidarPoint &p) { this->callbackStop(p); });
    CHECK(sx.scan() == kScanOk);
}

TEST_CASE_METHOD(CallbackFixture, "1.18", "[ScannerLVX]") {
    sx.setCallback([this](const LidarPoint &p) { this->callbackScan(p); });
    sx.scan();
    CHECK(scd == kScanError);
}

TEST_CASE_METHOD(CallbackFixture, "1.19", "[ScannerLVX]") {
    sx.setCallback([this](const LidarPoint &p) { this->callbackStop(p); });
    sx.scan();
    CHECK(!sx.isScanning());
}

TEST_CASE_METHOD(CallbackFixture, "1.20", "[ScannerLVX]") {
    sx.init();
    sx.stop();
    CHECK(sx.init());
}