Contributions to OSCATS are welcome!  You are encouraged to discuss ideas on
the [OSCATS mailing list](http://groups.google.com/group/oscats)
(oscats@googlegroups.com).  Submit patches to the mailing list as a
[Mercurial](http://mercurial.selenic.com/) bundle:
```
hg clone https://oscats.googlecode.com/hg/ oscats
cd oscats
# Edit, edit, edit.
hg commit
hg bundle my-spiffy-patch.hg
```
If you're new to Mercurial, read the amusing "[Hg Init: a Mercurial tutorial](http://hginit.com/)."

Note: [automake](http://www.gnu.org/software/automake/),
[autoconf](http://www.gnu.org/software/autoconf), and
[gtk-doc](http://www.gtk.org/gtk-doc) are required for building from the
repository.  Use the `autogen.sh` script to prepare the tree for building.