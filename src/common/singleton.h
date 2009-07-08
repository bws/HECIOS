#ifndef SINGLETON_H
#define SINGLETON_H
//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
