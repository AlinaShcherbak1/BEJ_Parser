#ifndef BEJ_TYPES_H
#define BEJ_TYPES_H

/**\file bej_types.h
 * Basic BEJ type tags (subset).
 */

enum bej_tag {
  BEJ_TAG_SET      = 0x30, /* subset: a set (object) */
  BEJ_TAG_INTEGER  = 0x10, /* subset: signed integer */
  BEJ_TAG_STRING   = 0x40, /* subset: UTF-8 string */
  BEJ_TAG_ARRAY    = 0x60, /* subset: array */

  BEJ_TAG_ANNOTATION = 0xF0 /* marker for stream header/annotation (simplified) */
};

#endif
