####################################################################################################
#
####################################################################################################


####################################################################################################
#Directories for library files 
####################################################################################################
vob_lib_dirs :=

####################################################################################################
#library files
####################################################################################################
vob_lib_files :=

####################################################################################################
#Directories for include files
####################################################################################################

####################################################################################################
#Directories for source files
####################################################################################################
vob_src_dirs := $(MED_VOICE_DIR)/voice_codec/fr/src

####################################################################################################
#Source files
####################################################################################################
vob_src_files :=$(vob_src_dirs)/fr_preproce.c    \
	$(vob_src_dirs)/fr_table.c       \
	$(vob_src_dirs)/fr_homing.c      \
	$(vob_src_dirs)/fr_interface.c   \
	$(vob_src_dirs)/fr_dec.c         \
	$(vob_src_dirs)/fr_enc.c         \
	$(vob_src_dirs)/fr_long_term.c   \
	$(vob_src_dirs)/fr_lpc.c         \
	$(vob_src_dirs)/fr_dtx.c         \
	$(vob_src_dirs)/fr_vad.c         \
	$(vob_src_dirs)/fr_rpe.c         \
	$(vob_src_dirs)/fr_short_term.c
	
####################################################################################################
#
####################################################################################################


####################################################################################################
#
####################################################################################################


####################################################################################################
#
####################################################################################################