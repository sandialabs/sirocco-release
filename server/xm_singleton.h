/* An interface that allows for an XM transport to be used by request
 * fulfilling routines while allowing the transport to be waited on
 * when it's idle.
 *
 * This has no guts anymore, as the original was designed only for
 * single-client operation.
 */

struct xm_transport;
struct xms_xp;

int xms_start(struct xm_transport *xmx);
#define xms_get() xms_getl(__FILE__, __LINE__)
#define xms_put() xms_putl(__FILE__, __LINE__)
void xms_getl(const char *filename, int line);
void xms_putl(const char *filename, int line);
int xms_stop(void);

