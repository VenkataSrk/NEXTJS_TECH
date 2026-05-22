#include <stdio.h>
#include <sofia-sip/nua.h>

void send_message (nua_t *nua);

void event_callback(nua_event_t   event,
                    int           status,
                    char const   *phrase,
                    nua_t        *nua,
                    nua_magic_t  *magic,
                    nua_handle_t *nh,
                    nua_hmagic_t *hmagic,
                    sip_t const  *sip,
                    tagi_t        tags[]);

int
main (int argc, char *argv[])
{
  su_root_t *root;
  nua_t *nua;

  /* Initialize Sofia-SIP library and create event loop */

  su_init ();
  root = su_root_create (NULL);

  /* Create a user agent instance. Caller and callee should bind to different
   * address to avoid conflicts. The stack will call the 'event_callback()' 
   * callback when events such as succesful registration to network, 
   * an incoming call, etc, occur. 
   */
  nua = nua_create(root, /* Event loop */
                   event_callback, /* Callback for processing events */
                   NULL, /* Additional data to pass to callback */
#ifdef CALLER
           NUTAG_URL("sip:0.0.0.0:5062"), /* Address to bind to */
#else
           NUTAG_URL("sip:0.0.0.0:5060"),
#endif
                   TAG_END()); /* Last tag should always finish the sequence */

#ifdef CALLER
  send_message (nua);
#endif

  /* Run event loop */
  su_root_run (root);

  /* Destroy allocated resources */
  nua_destroy (nua);
  su_root_destroy (root);
  su_deinit ();

  return 0;
}

/* This callback will be called by SIP stack to
 * process incoming events 
 */
void event_callback(nua_event_t   event,
                    int           status,
                    char const   *phrase,
                    nua_t        *nua,
                    nua_magic_t  *magic,
                    nua_handle_t *nh,
                    nua_hmagic_t *hmagic,
                    sip_t const  *sip,
                    tagi_t        tags[])
{
  printf ("Hello world!\n"
          "I have received an event %s status %d %s\n", 
          nua_event_name (event), status, phrase);
}

/* Create a communication handle, send MESSAGE with it and destroy it */
void send_message (nua_t *nua)
{
  nua_handle_t *handle;

  handle = nua_handle(nua, NULL, SIPTAG_TO_STR("sip:test@127.0.0.1:5060"), TAG_END());

  nua_message(handle,
          SIPTAG_CONTENT_TYPE_STR("text/plain"),
          SIPTAG_PAYLOAD_STR("Hello callee!!!"),
          TAG_END());

  nua_handle_destroy (handle);
}
