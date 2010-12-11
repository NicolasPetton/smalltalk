MthWrapper_FILES = \
packages/mth-wrapper/mth-wrapper.st packages/mth-wrapper/mth-example.st 
$(MthWrapper_FILES):
$(srcdir)/packages/mth-wrapper/stamp-classes: $(MthWrapper_FILES)
	touch $(srcdir)/packages/mth-wrapper/stamp-classes
