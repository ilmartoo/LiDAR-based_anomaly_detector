/**
 * @file anomaly_test.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 10/05/2022
 *
 * Test para el módulo de la CLI
 *
 */

#include "catch.hpp"
#include "catch_utils.hh"

#include <vector>

#include "object_characterization/ObjectCharacterizer.hh"
#include "object_characterization/CharacterizedObject.hh"
#include "object_characterization/DBScan.hh"

#include "scanner/IScanner.hh"
#include "models/LidarPoint.hh"

/* MOCKUP */
class ScannerMock : public IScanner {
   public:
    std::function<void(const LidarPoint &p)> func;
    std::vector<LidarPoint> v;

    ScannerMock() {
        v.reserve(400 * 6 + 2);
        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                v.push_back({{0, 0}, 0, 0, i, j});
                v.push_back({{0, 0}, 0, 100, i, j});
                v.push_back({{0, 0}, 0, i, 0, j});
                v.push_back({{0, 0}, 0, i, 100, j});
                v.push_back({{0, 0}, 0, i, j, 0});
                v.push_back({{0, 0}, 0, i, j, 100});
            }
        }
        v.push_back({{0, 0}, 0, -100, -100, -100});
        v.push_back({{2, 0}, 0, 0, 0, 0});
    }

    bool init() { return true; }
    ScanCode scan() {
        if (func) {
            for (auto &e : v) {
                func(e);
            }
        }
        return kScanOk;
    }
    void pause() {}
    bool setCallback(std::function<void(const LidarPoint &p)> func) { return (bool)(this->func = func); }
    void stop() {}
};

class ScannerMockBad : public IScanner {
   public:
    std::function<void(const LidarPoint &p)> func;
    std::vector<LidarPoint> v;

    ScannerMockBad() {
        v.reserve(400 * 6 + 2);
        for (int i = 0; i < 2000; i += 100) {
            for (int j = 0; j < 2000; j += 100) {
                v.push_back({{0, 0}, 0, 0, i, j});
                v.push_back({{0, 0}, 0, 100, i, j});
                v.push_back({{0, 0}, 0, i, 0, j});
                v.push_back({{0, 0}, 0, i, 100, j});
                v.push_back({{0, 0}, 0, i, j, 0});
                v.push_back({{0, 0}, 0, i, j, 100});
            }
        }
        v.push_back({{0, 0}, 0, -100, -100, -100});
        v.push_back({{2, 0}, 0, 0, 0, 0});
    }

    bool init() { return false; }
    ScanCode scan() {
        if (func) {
            for (auto &e : v) {
                func(e);
            }
        }
        return kScanOk;
    }
    void pause() {}
    bool setCallback(std::function<void(const LidarPoint &p)> func) { return (bool)(this->func = func); }
    void stop() {}
};

class ScannerMockWait : public IScanner {
   public:
    std::function<void(const LidarPoint &p)> func;
    std::vector<LidarPoint> v;
    unsigned discarded;

    ScannerMockWait() : discarded(0) {
        v.push_back({{0, 0}, 0, 0, 0, 0});
        v.push_back({{1, 0}, 0, 0, 0, 0});
        v.push_back({{2, 0}, 0, 0, 0, 0});
    }

    bool init() { return true; }
    ScanCode scan() {
        if (func) {
            for (size_t i = 0; i < v.size() && scanning; ++i) {
                func(v[i]);
                ++discarded;
            }
        }
        return kScanOk;
    }
    void pause() {}
    bool setCallback(std::function<void(const LidarPoint &p)> func) { return (bool)(this->func = func); }
    void stop() {}
};
/**********/

class CharacterizationFixture {
   public:
    ScannerMock sg;
    ScannerMockBad sb;
    ScannerMockWait sw;

    ObjectCharacterizer ocg;
    ObjectCharacterizer ocb;
    ObjectCharacterizer ocw;

	std::vector<Point> cubo;

    CharacterizationFixture() : sg(),
                                sb(),
                                ocg(&sg, 10, 10, 0, 10, true),
                                ocb(&sb, 10, 10, 0, 10, false),
                                ocw(&sw, 10, 10, 0, 10, false) {
        sg.setCallback([this](const LidarPoint &p) { this->ocg.newPoint(p); });
        sb.setCallback([this](const LidarPoint &p) { this->ocb.newPoint(p); });
        sb.setCallback([this](const LidarPoint &p) { this->ocw.newPoint(p); });

		cubo.reserve(400 * 6 + 1);
        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                cubo.push_back({0, i, j});
                cubo.push_back({100, i, j});
                cubo.push_back({i, 0, j});
                cubo.push_back({i, 100, j});
                cubo.push_back({i, j, 0});
                cubo.push_back({i, j, 100});
            }
        }
        cubo.push_back({-100, -100, -100});
    }
};

TEST_CASE_METHOD(CharacterizationFixture, "3.1, 3.2, 3.3", "[ObjectCharacterizer]") {
    ocg.defineBackground();

    // 3.1
    CHECK(ocg.init());
    // 3.2
    CHECK(!ocb.init());
    // 3.3
    CHECK(!ocg.defineObject().first);
}

TEST_CASE_METHOD(CharacterizationFixture, "3.4, 3.5", "[ObjectCharacterizer]") {
    // 3.4
    CHECK(!ocb.defineObject().first);
    // 3.5
    CHECK(ocg.defineObject().first);
}

TEST_CASE_METHOD(CharacterizationFixture, "3.6", "[ObjectCharacterizer]") {
    ocw.wait(1500);

    CHECK(sw.discarded == 2);
}

TEST_CASE_METHOD(CharacterizationFixture, "3.7, 3.8", "[DBScan]") {
    // 3.7
    CHECK(DBScan::clusters(cubo).size() == 1);
    // 3.8
    CHECK(DBScan::normals(cubo).size() >= 6);
}