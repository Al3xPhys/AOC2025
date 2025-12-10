#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/* The input file contains coordinates in the format "x,y" on each line. These
   represent red tiles. You can choose any two red tiles (#) as the opposite
   corners of your rectangle; your goal is to find the largest rectangle
   possible. In the list, every red tile (#) is connected to the red tile before
   and after it by a straight line of green tiles (X). The list wraps, so the
   first red tile is also connected to the last red tile. Tiles that are
   adjacent in your list will always be on either the same row or the same
   column. In addition, all of the tiles inside this loop of red and green tiles
   are also green. The loop makes a closed shape with no holes (i.e a polygon)
        The green tiles (X) are connected to each other horizontally and
   vertically (not diagonally). The polygon can be concave or convex. The
   remaining tiles are never red nor green (.). The rectangle you choose still
   must have red tiles in opposite corners, but any other tiles it includes must
   now be red or green. This significantly limits your options. All four corners
   of the rectangle must be on or inside the polygon formed by the red and green
   tiles. The rectangle is aligned to the x and y axes. The rectangle edges
   cannot pass outside the polygon formed by the red and green tiles, but they
   can touch the edges. Using two red tiles as opposite corners, what is the
   largest area of any rectangle you can make using only red and green tiles?
*/

struct Point {
  int64_t x;
  int64_t y;
};

vector<Point> parseInput(ifstream &infile) {
  vector<Point> data;
  string line;
  while (getline(infile, line)) {
    size_t comma_pos = line.find(',');
    if (comma_pos != string::npos) {
      int64_t x = stoll(line.substr(0, comma_pos));
      int64_t y = stoll(line.substr(comma_pos + 1));
      data.emplace_back(x, y);
    }
  }
  return data;
}

bool isPointInPolygon(const Point &point,
                      const vector<Point> &vertex_coordinates) {
  // Implement point-in-polygon algorithm (e.g., ray-casting or winding number)
  // to check if point is inside the polygon defined by vertex_coordinates.
  int n = vertex_coordinates.size(); // Number of vertices in the polygon
  int count = 0; // count how many times a ray intersects polygon edges

  // iterate through each edge of the polygon
  for (int i = 0; i < n; ++i) {
    Point p1 = vertex_coordinates[i];
    // next vertex, wrapping around, ensure the last vertex connects to first
    Point p2 = vertex_coordinates[(i + 1) % n];

    // check if the point lies exactly on the edge
    int64_t minX = min(p1.x, p2.x);
    int64_t maxX = max(p1.x, p2.x);
    int64_t minY = min(p1.y, p2.y);
    int64_t maxY = max(p1.y, p2.y);
    if (point.x >= minX && point.x <= maxX && point.y >= minY &&
        point.y <= maxY) {
      // the point is within the bounding box of the edge
      if (p1.x == p2.x || p1.y == p2.y ||
          (point.y - p1.y) * (p2.x - p1.x) ==
              (point.x - p1.x) * (p2.y - p1.y)) {
        return true; // Point is on the edge (collinear)
      }
    }

    // check if the points y-coordinates are within the edge's y-range AND if
    // the point is to the left of the edge
    if ((point.y > min(p1.y, p2.y)) && (point.y <= max(p1.y, p2.y)) &&
        (point.x <= max(p1.x, p2.x))) {
      // calculate the x-coordinate of the intersection of the edge with the
      // horizontal ray from the line through the point
      int64_t xIntersect =
          (point.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
      // if the edge is vertical or the point's x-coordinate is less than or
      // equal to the intersection x-coordinate, count the intersection
      if (p1.x == p2.x || point.x <= xIntersect) {
        count++;
      }
    }
  }
  // if the count of intersections is odd, the point is inside the polygon
  return (count % 2 == 1); // odd count means inside
}

// Helper function to calculate cross product of vectors (b-a) and (c-a)
int64_t crossProduct(const Point &a, const Point &b, const Point &c) {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// Helper function to check if point q lies on segment pr (assuming collinear)
bool onSegment(const Point &p, const Point &q, const Point &r) {
  return q.x >= min(p.x, r.x) && q.x <= max(p.x, r.x) && q.y >= min(p.y, r.y) &&
         q.y <= max(p.y, r.y);
}

bool doSegmentsIntersect(const Point &p1, const Point &p2, const Point &p3,
                         const Point &p4) {
  // Calculate cross products
  int64_t d1 = crossProduct(p3, p4, p1); // (p4-p3) x (p1-p3)
  int64_t d2 = crossProduct(p3, p4, p2); // (p4-p3) x (p2-p3)
  int64_t d3 = crossProduct(p1, p2, p3); // (p2-p1) x (p3-p1)
  int64_t d4 = crossProduct(p1, p2, p4); // (p2-p1) x (p4-p1)

  // General case: segments intersect if points are on opposite sides
  if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
      ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
    return true;
  }

  // Special cases: collinear points
  if (d1 == 0 && onSegment(p3, p1, p4))
    return true;
  if (d2 == 0 && onSegment(p3, p2, p4))
    return true;
  if (d3 == 0 && onSegment(p1, p3, p2))
    return true;
  if (d4 == 0 && onSegment(p1, p4, p2))
    return true;

  return false;
}

bool doesLineIntersectPolygon(const Point &p1, const Point &p2,
                              const vector<Point> &vertex_coordinates) {
  /* define line segment p1 to p2, iterate through polygon edges where the edge
    segement is from vi to vi+1, with vn = v0 for the last edge. check for
    intersection with edge (cross product method) to see if (p1,p2) intersects
    polygon (vi,vi+1)
      For each edge (A,B) of polyogn, check sign of cross product (B-A) x (P1-A)
    and (B-A) * (P2-A). If signs are different, P1 and P2 are on opposite sides
    of the line containing the edge (A,B). This means segment MIGHT cross the
    that edge. Need to also confirm that intersection point lies within both
    segments. */

  int n = vertex_coordinates.size(); // Number of vertices in the polygon
  for (int i = 0; i < n; ++i) {
    Point v1 = vertex_coordinates[i];
    Point v2 = vertex_coordinates[(i + 1) % n];

    // check if segements are collinear (lying on the same line)
    int64_t cross1 = crossProduct(v1, v2, p1);
    int64_t cross2 = crossProduct(v1, v2, p2);

    if (cross1 == 0 && cross2 == 0) {
      // both endpoints are collinear with edge, check if they lie on the edge
      continue; // allowed, so skip to next edge
    }

    // if endpoints are on opposite sides of the edge line, its a crossing
    if ((cross1 > 0 && cross2 < 0) || (cross1 < 0 && cross2 > 0)) {
      // now confirm the segments actually intersect
      int64_t cross3 = crossProduct(p1, p2, v1);
      int64_t cross4 = crossProduct(p1, p2, v2);

      // only crossing if polygon edge endppints are also on opposite sides of
      // line
      if ((cross3 > 0 && cross4 < 0) || (cross3 < 0 && cross4 > 0)) {
        return true; // segments intersect properly
      }
    }
  } // if we get here, no intersections found
  return false; // no crossing found
}

int64_t make_rectangle_area_per_pair(const vector<Point> &vertex_coordinates) {
  /* iterate over pairs of vertices to calculate rectangle areas */
  int_fast64_t max_area = 0; // Store the maximum area found
  for (size_t i = 0; i < vertex_coordinates.size(); ++i) {
    for (size_t j = i + 1; j < vertex_coordinates.size(); ++j) {

      // get two chosen corners
      const auto &[x1, y1] = vertex_coordinates[i];
      const auto &[x2, y2] = vertex_coordinates[j];
      // get the other two corners
      Point corner3 = {x1, y2};
      Point corner4 = {x2, y1};
      // calculate area
      int64_t width = abs(x2 - x1) + 1;  // +1 to include the starting tile
      int64_t height = abs(y2 - y1) + 1; // +1 to include the starting tile
      int64_t area = width * height;

      // if the area is more than current max area and both other corners are
      // inside and the edges do not intersect polygon, update max area
      if ((area >= max_area) &&
          (isPointInPolygon(corner3, vertex_coordinates)) &&
          (isPointInPolygon(corner4, vertex_coordinates)) &&
          (!doesLineIntersectPolygon({x1, y1}, {x1, y2}, vertex_coordinates)) &&
          (!doesLineIntersectPolygon({x1, y2}, {x2, y2}, vertex_coordinates)) &&
          (!doesLineIntersectPolygon({x2, y2}, {x2, y1}, vertex_coordinates)) &&
          (!doesLineIntersectPolygon({x2, y1}, {x1, y1}, vertex_coordinates))) {
        max_area = area; // update maximum area
        cout << "New max area: " << max_area << " with corners: (" << x1 << ","
             << y1 << ") and (" << x2 << "," << y2 << ")" << endl;
      } else {
      }
    }
  }
  return max_area;
}

int main() {
  ifstream infile("input.txt");
  if (!infile) {
    cerr << "Unable to open file input.txt";
    return 1; // Exit with error code
  }
  vector<Point> vertex_coordinates = parseInput(infile);

  int64_t max_area = make_rectangle_area_per_pair(vertex_coordinates);
  cout << "Maximum rectangle area: " << max_area << endl;
  return 0; // Exit successfully
}