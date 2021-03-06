######################################################################################
##                                                                                  ##
## AUTHOR  : Aby Louw                                                               ##
## DATE    : 5 November 2009                                                        ##
##                                                                                  ##
######################################################################################
##                                                                                  ##
## Source files for Addendum JSON formatter plug-in                                 ##
##                                                                                  ##
##                                                                                  ##
######################################################################################


######## source files ##################

speect_plugin_sources(
  src/plugin.c
  src/addendum_json.c
  src/serialized_addendum.c
  src/read.c
  )
 

######## header files ##################

speect_plugin_headers(
  src/addendum_json.h
  src/serialized_addendum.h
  )

