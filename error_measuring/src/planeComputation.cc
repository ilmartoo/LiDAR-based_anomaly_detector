#include "armadillo"

#include "models/Point.hh"

Point computeCentroid(const std::vector<Point *> &points) {
  double x{}, y{}, z{};
  size_t numPoints = points.size();

  for (const auto &p : points) {
    x += p->getX();
    y += p->getY();
    z += p->getZ();
  }

  x /= numPoints;
  y /= numPoints;
  z /= numPoints;

  return Point(x, y, z);
}

void computeSVD(const std::vector<Point *> &points, arma::mat &U, arma::vec &s,
                arma::mat &V) {
  mat P(3, points.size());

  size_t idx = 0;

  for (const auto &p : points) {
    P(0, idx) = p->getX();
    P(1, idx) = p->getY();
    P(2, idx) = p->getZ();
    ++idx;
  }

  arma::vec = arma::mean(P, 1);
  P.row(0) -= mean(0);
  P.row(1) -= mean(1);
  P.row(2) -= mean(2);

  // SVD
  svd_econ(U, s, V, P);
}

arma::vec computeNormal(const std::vector<Point *> &points) {
  arma::vec normal(3);

  // SVD
  arma::mat U, V;
  arma::vec s;
  computeSVD(points, U, s, V);

  normal = U.col(2);

  return normal;
}

arma::vec computePlane(vec &normal, vec &centroid) {
  vec plane(4);

  plane[0] = normal[0];
  plane[1] = normal[1];
  plane[2] = normal[2];
  plane[3] = (-plane[0] * centroid[0] + plane[1] * centroid[1] +
              plane[2] * centroid[2]);

  return plane;
}