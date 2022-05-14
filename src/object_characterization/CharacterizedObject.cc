/**
 * @file CharacterizedObject.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 13/05/2022
 *
 * Implementación del objeto CharacterizedObject
 *
 */

#include <queue>
#include <set>
#include <map>
#include <utility>
#include <fstream>

#include "object_characterization/CharacterizedObject.hh"
#include "object_characterization/PlaneUtils.hh"
#include "models/OctreeMap.hh"
#include "models/Point.hh"

CharacterizedObject::CharacterizedObject(const OctreeMap &om)
    : bbox(om.getMap().getCenter(),
           om.getMap().getMax().getX() - om.getMap().getMin().getX(),
           om.getMap().getMax().getY() - om.getMap().getMin().getY(),
           om.getMap().getMax().getZ() - om.getMap().getMin().getZ()),
      faces() {
    std::queue<Point> pending;
    std::set<std::string> visited;

    pending.push(*om.getPoints().begin());
    do {
        Point &p = pending.front();

        // Comprobamos si hemos visitado el punto
        if (visited.insert(p.ID()).second) {
            std::vector<Point *> neighbours = om.getMap().searchNeighbors(p, 0.1, Kernel_t::sphere);
            neighbours.push_back(&p);

            // No calculamos la normal para outliers
            if (neighbours.size() > 3) {
                Vector normal = PlaneUtils::computeNormal(neighbours);
                if (normal[0] < 0.0) {
                    normal = normal * -1.0;
                }

                // Añadimos el punto con normal nueva o insertamos punto en vector ya existente
                if (!faces.insert({normal, {1, p}}).second) {
                    faces.find(normal)->second.push_back(p);
                }

                // Añadimos puntos vecinos a pendientes
                for (auto &p : neighbours) {
                    pending.push(*p);
                }
            }
        }

        pending.pop();
    } while (!pending.empty());
}

bool CharacterizedObject::write(const std::string &filename) {
    std::ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile.write((char *)&bbox, sizeof(BBox));  // Bounding box
        size_t len = faces.size();
        outfile.write((char *)&len, sizeof(size_t));  // Numero de caras

        for (auto &f : faces) {
            outfile.write((char *)&f.first, sizeof(Vector));          // Vector normal de la cara
            len = f.second.size();
            outfile.write((char *)&len, sizeof(size_t));  // Numero de puntos de la cara

            for (auto &p : f.second) {
                outfile.write((char *)&p, sizeof(Point));  // Punto de la cara
            }
        }

        outfile.close();
        return !outfile.fail();

    } else {
        return false;
    }
}

std::pair<bool, CharacterizedObject> CharacterizedObject::load(const std::string &filename) {
    std::ifstream infile(filename);
    if (infile.is_open()) {
        size_t nfaces, npoints;
        BBox bbox;
        infile.read((char *)&bbox, sizeof(BBox));      // Bounding box
        infile.read((char *)&nfaces, sizeof(size_t));  // Numero de caras

        std::map<Vector, std::vector<Point>> faces;
        Vector normal;
        Point point;
        for (unsigned i = 0; i < nfaces; ++i) {
            infile.read((char *)&normal, sizeof(Vector));  // Vector normal de la cara
            std::vector<Point> &vref = faces.insert({normal, {}}).first->second;

            infile.read((char *)&npoints, sizeof(size_t));  // Numero de puntos de la cara
            for (unsigned j = 0; j < npoints; ++j) {
                infile.read((char *)&point, sizeof(Point));  // Punto de la cara
                vref.push_back(point);
            }
        }

        infile.close();
        return {!infile.fail(), CharacterizedObject(bbox, faces)};

    } else {
        return {false, {}};
    }
}