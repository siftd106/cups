#include <ruby_cups.h>

cups_dest_t *dests, *dest;
VALUE rubyCups, printJobs;

/*
* call-seq:
*   Cups.show_destinations -> Array
*
* Show all destinations on the default server
*/
static VALUE cups_show_dests(VALUE self)
{
  VALUE dest_list;
  int i;
  int num_dests = cupsGetDests(&dests); // Size of dest_list array
  dest_list = rb_ary_new2(num_dests);
  
  for (i = num_dests, dest = dests; i > 0; i --, dest ++) {
    VALUE destination = rb_str_new2(dest->name);
    rb_ary_push(dest_list, destination); // Add this testination name to dest_list string
  }
  
  cupsFreeDests(num_dests, dests);  
  return dest_list;
}

/*
* call-seq:
*   Cups.default_printer -> String or nil
*
* Get default printer or class. Returns a string or false if there is no default
*/
static VALUE cups_get_default(VALUE self)
{
  const char *default_printer;
  default_printer = cupsGetDefault();

  if (default_printer != NULL) {
    VALUE def_p = rb_str_new2(default_printer);

    return def_p;
  } else {
    return Qnil;
  }
}

/*
 * call-seq:
 *   Cups.options_for(name) -> Hash or nil
 *
 * Get all options from CUPS server with name. Returns a hash with key/value pairs
 * based on server options, or nil if no server with name.
 */
static VALUE cups_get_options(VALUE self, VALUE printer)
{
  // Don't have to lift a finger unless the printer exists.
  if (!printer_exists(printer)){
    rb_raise(rb_eRuntimeError, "The printer or destination doesn't exist!");
  }

  VALUE options_list;
  int i;
  char *printer_arg = RSTRING_PTR(printer);

  options_list = rb_hash_new();

  cups_dest_t *dests;
  int num_dests = cupsGetDests(&dests);
  cups_dest_t *dest = cupsGetDest(printer_arg, NULL, num_dests, dests);

  if (dest == NULL) {
    cupsFreeDests(num_dests, dests);    
    return Qnil;
  } else {
    for(i =0; i< dest->num_options; i++) {
      rb_hash_aset(options_list, rb_str_new2(dest->options[i].name), rb_str_new2(dest->options[i].value));
    }
    cupsFreeDests(num_dests, dests);
    return options_list;
  }

}

/*
*/

void Init_cups() {
  rubyCups = rb_define_module("Cups");
  
  // Cups Module Methods
  rb_define_singleton_method(rubyCups, "show_destinations", cups_show_dests, 0);
  rb_define_singleton_method(rubyCups, "default_printer", cups_get_default, 0);
  rb_define_singleton_method(rubyCups, "options_for", cups_get_options, 1);
}