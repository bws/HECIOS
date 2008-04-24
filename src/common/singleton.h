#ifndef SINGLETON_H
#define SINGLETON_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

/**
 * Template that provides standard singleton access to a class.  To use this
 * template, inherit from the template specialized on the derived class.
 *
 * E.g.
 *
 * class UniqueFactory : public Singleton<UniqueFactory> {};
 *
 * Requires a protected default constructor and destructor
 *
 */
template<class T>
class Singleton
{
public:
    /**
     * @return singleton of type T
     *
     * Requires a protected default constructor
     */
    static T& instance();

    /** Clears the singleton state */
    static void clearState();

private:
    /** Singleton pointer */
    static T* instance_;
};

template<class T> T* Singleton<T>::instance_ = 0;

template<class T>
T& Singleton<T>::instance()
{
    if (0 == instance_)
    {
        instance_ = new T();
    }
    return *instance_;
}

template<class T>
void Singleton<T>::clearState()
{
    delete instance_;
    instance_ = 0;
}

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
