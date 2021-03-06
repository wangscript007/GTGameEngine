// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ParticleList
#define GT_ParticleList

#include "Particle.hpp"

namespace GT
{
    /// Container class specially designed for efficient storing of particles.
    ///
    /// This container can probably be used for other objects as well, in which case this may be turned into a template container.
    ///
    /// In a particle effect, there will potential for many short-lived particles, each of which will be added and removed from this
    /// list. So therefore, adding and removing particles from a list needs to be efficient. We can use this list for doing the
    /// construction of particles.
    class ParticleList
    {
    public:

        /// Constructor.
        ParticleList(size_t bufferSize = 0);

        /// Copy constructor.
        ParticleList(const ParticleList &other);

        /// Destructor.
        ~ParticleList();


        /// Pushes a new particle to the end of the buffer, returning a reference to the new particle.
        Particle & PushNewParticle();

        /// Removes the particle at the given index.
        void Remove(size_t index);



        /// Retrieves the particle count.
        size_t GetCount() const;
        
        /// Retrieves the particle at the given index.
        ///
        /// @param index [in] The index of the particle to retrieve.
        ///
        /// @remarks
        ///     The returned value can not be considered valid when a new item is added or removed.
              Particle & GetParticle(size_t index);
        const Particle & GetParticle(size_t index) const;


        /// Clears the list.
        void Clear();


        /// Retrieves the size of the internal buffer, in Particles.
        size_t GetBufferSize() const;

        /// Resizes the internal buffer, making sure everything is copied over appropriately.
        ///
        /// @param [in] The new size of the internal buffer, in Particles.
        ///
        /// @remarks
        ///     This does not modify the particle count, only the size of the internal buffer.
        void Resize(size_t newBufferSize);

        
        
    private:

        /// A pointer to the buffer containing the particles.
        Particle* buffer;

        /// The size of the buffer, in "Particle"s.
        size_t bufferSize;

        /// The number of particles currently in the list.
        size_t count;

        /// The index of the first particle.
        size_t startIndex;
        
        
    private:    // No assignment, yet.
        ParticleList & operator=(const ParticleList &);
    };
}

#endif
