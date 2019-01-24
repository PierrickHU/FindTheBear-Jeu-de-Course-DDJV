#ifndef INCOPIABLE_H
#define INCOPIABLE_H

struct Incopiable {
   // on dit explicitement que les fonction n'existent pas
   Incopiable(const Incopiable&) = delete;
   Incopiable& operator= (const Incopiable&) = delete;

protected:
   // on dit explicitement au compilateur de prendre son comportement par defaut
   Incopiable() = default;
   ~Incopiable() = default;
};

#endif

