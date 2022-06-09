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
#include "scanner/ScannerLVX.hh"
#include "scanner/ScannerCSV.hh"

#include "models/LidarPoint.hh"

class CallbackFixture {
   public:
    static const unsigned LIMIT = 100;
    unsigned count;
    LidarPoint pts[LIMIT];
    IScanner *inst;

    CallbackFixture() : count(0), pts(), inst(nullptr) {}

    void callback100(const LidarPoint &p) {
        if (count >= LIMIT) {
            return;
        }
        pts[count++] = p;
    }

    void callbackEOF(const LidarPoint &p) {
        ++count;
    }
};

TEST_CASE_METHOD(CallbackFixture, "scanner csv", "[scanner]") {
    inst = ScannerCSV::create("../../test/scanner/test_data.csv");
    inst = IScanner::getInstance();

    uint32_t rresult = 45;
    Timestamp tresult("342307696760");
    LidarPoint presult(tresult, rresult, 2042, 1559, -2);

    inst->setCallback(([this](const LidarPoint &p) { this->callback100(p); }));
    inst->init();
    inst->scan();
    inst->stop();
    
    CHECK_MESSAGE((presult == pts[73] && rresult == pts[73].getReflectivity() && tresult == pts[73].getTimestamp()), "Scaner do not scan correctly");
}

TEST_CASE_METHOD(CallbackFixture, "scanner csv read to end", "[scanner]") {
    inst = ScannerCSV::create("../../test/scanner/test_data.csv");

    inst->setCallback(([this](const LidarPoint &p) { this->callbackEOF(p); }));
    inst->init();
    ScanCode scan1result = inst->scan();
    inst->setCallback(([this](const LidarPoint &p) { this->callback100(p); }));
    ScanCode scan2result = inst->scan();

    CHECK_MESSAGE((scan1result == kScanEof && scan2result == kScanOk), "Scaner do not scan correctly");
}

TEST_CASE_METHOD(CallbackFixture, "scanner csv no file", "[scanner]") {
    inst = ScannerCSV::create("");

    inst->setCallback(([this](const LidarPoint &p) { this->callback100(p); }));
    bool initresult = inst->init();
    ScanCode scanresult = inst->scan();

    CHECK_MESSAGE((!initresult && scanresult == kScanError), "Scanner works with inexistent file");
}