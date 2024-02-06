#ifndef SQLITE__COLLATION__H
#define SQLITE__COLLATION__H

namespace sqlite {
    class Collation {
    public:
        virtual ~Collation();

        Collation(const Collation &other) = delete;
        Collation &operator =(const Collation &other) = delete;

        virtual const char *name() const = 0;
        virtual int textRepresentation() const = 0;
        virtual int compare(int string1Length, const void *string1, int string2Length, const void *string2) = 0;
    };
}

#endif
