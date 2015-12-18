#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

#define PRINT_STATUS(force, prepend, ...)			\
	fprintf(stderr, prepend __FILE__ ","                    \
		STRINGIFY(__LINE__)                             \
		": " __VA_ARGS__)

#define INFO(...) do {							\
		PRINT_STATUS(1, "LIBASG INFO @", __VA_ARGS__);	\
        } while(0)

#define WARN(...) do {							\
		PRINT_STATUS(1, "LIBASG WARNING @", __VA_ARGS__);	\
        } while(0)

#define ERR(...) do {						\
		PRINT_STATUS(1, "LIBASG ERROR @", __VA_ARGS__);	\
        } while(0)
