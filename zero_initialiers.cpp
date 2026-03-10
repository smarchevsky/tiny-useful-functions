// clang-format off
#include <new>
#include <memory>

//
// STACK
//

// zero-initializes a stack object if it has constructor like: Foo() {}  with RAII deleter
template <class T>
union zero_initializer {
private:
    char dummy[sizeof(T)];
    T instance;

public:
    operator T&() { return instance; }
    operator const T&() const { return instance; }

    template <typename... Args>
    zero_initializer(Args&&... args) : dummy { 0 } { new (&instance) T(std::forward<Args>(args)...); }
    ~zero_initializer() { instance.~Foo(); }
};

#define ZEROINIT(Type, Name, ...) Type& Name = zero_initializer<Type>(##__VA_ARGS__)
// ZEROINIT(Foo, foo, 3.14);

//
// HEAP
//

// zero-initialize unique_ptr
template <typename T, typename Deleter = std::default_delete<T>, typename... Args>
std::unique_ptr<T, Deleter> make_zeroed_unique(Args&&... args)
{
    void* raw = ::operator new(sizeof(T), std::align_val_t { alignof(T) });
    std::memset(raw, 0, sizeof(T));
    return std::unique_ptr<T, Deleter>(new (raw) T(std::forward<Args>(args)...));
}
