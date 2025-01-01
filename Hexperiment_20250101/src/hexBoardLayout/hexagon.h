#pragma once

// a hexboard obviously needs to have a class of hexagon coordinates.
// https://www.redblobgames.com/grids/hexagons/ for more details.
//
//                     | -y axis
//              [-1,-1] [ 1,-1]
//  -x axis [-2, 0] [ 0, 0] [ 2, 0]  +x axis
//              [-1, 1] [ 1, 1]
//                     | +y axis

struct hex_t { 
	int x;      
	int y;
	hex_t(int x=0, int y=0) : x(x), y(y) {}
  // overload the = operator
  hex_t& operator=(const hex_t& rhs) {
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
  // two hexes are == if their coordinates are ==
	bool operator==(const hex_t& rhs) const {
		return (x == rhs.x && y == rhs.y);
	}
  // left-to-right, top-to-bottom order
  bool operator<(const hex_t& rhs) const {
    if (y == rhs.y) {
      return (x < rhs.x);
    } else {
      return (y < rhs.y);
    }
  }
  // you can + two hexes by adding the coordinates
	hex_t operator+(const hex_t& rhs) const {
		return hex_t(x + rhs.x, y + rhs.y);
	}
  // you can * a hex by a scalar to multi-step
	hex_t operator*(const int& rhs) const {
		return hex_t(rhs * x, rhs * y);
	}
  // subtraction is + hex*-1
  hex_t operator-(const hex_t& rhs) const {
        return *this + (rhs * -1);
    }
};
// dot product of two vectors (i.e. distance & # of musical steps per direction)
int dot_product(const hex_t& A, const hex_t& B) {
    return (A.x * B.x) + (A.y * B.y);
}
// keep this as a non-class enum because
// we need to be able to cycle directions
enum {
	dir_e = 0,
	dir_ne = 1,
	dir_nw = 2,
	dir_w = 3,
	dir_sw = 4,
	dir_se = 5
};
hex_t unitHex[] = {
  // E       NE      NW      W       SW      SE
  { 2, 0},{ 1,-1},{-1,-1},{-2, 0},{-1, 1},{ 1, 1}
};