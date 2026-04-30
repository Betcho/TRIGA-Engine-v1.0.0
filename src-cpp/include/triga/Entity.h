class Entity {
public:
    // ... existing code ...
    
    // Transform getters - MUST ADD THESE
    Vector3 getPosition() const { return m_transform.position; }
    Vector3 getRotation() const { return m_transform.rotation; }
    Vector3 getScale() const { return m_transform.scale; }
    
    // Transform setters
    void setPosition(const Vector3& pos) { m_transform.position = pos; }
    void setRotation(const Vector3& rot) { m_transform.rotation = rot; }
    void setScale(const Vector3& scl) { m_transform.scale = scl; }
};
