#ifndef QHASH_PTR_H
#define QHASH_PTR_H

template <typename T>
static unsigned int qHash(const std::shared_ptr<T>& ptr, unsigned int seed = 0)
{
	return qHash(ptr.get(), seed);
}

#endif // QHASH_PTR_H
