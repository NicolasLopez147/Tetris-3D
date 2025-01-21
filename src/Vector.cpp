class Vector3D {
public:
    // Public member variables representing the 3D coordinates
    float x, y, z;

    // Constructor: Initializes the vector with default or given coordinates
    Vector3D(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

    // Overloaded operator+ to add two vectors component-wise
    Vector3D operator+(const Vector3D& other) const {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }

    // Overloaded operator- to subtract two vectors component-wise
    Vector3D operator-(const Vector3D& other) const {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }

    // Overloaded operator/ to divide the vector components by a scalar
    Vector3D operator/(float scalar) const {
        return Vector3D(x / scalar, y / scalar, z / scalar);
    }

    // Overloaded operator* to multiply the vector components by a scalar
    Vector3D operator*(float scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    // Overloaded operator== to compare two vectors for equality
    bool operator==(const Vector3D& other) {
        return x == other.x && y == other.y && z == other.z;
    }
};
