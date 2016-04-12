Have a question that isn't covered here?  Submit it to the OSCATS
[mailing list](http://groups.google.com/group/oscats)
(oscats@googlegroups.com).



# General #

## 1. What is Computerized Adaptive Testing and why should I care? ##

[Computerized Adaptive Testing](http://en.wikipedia.org/wiki/Computerized_adaptive_testing)
(CAT) is a form of assessment delivered by computer in which the next item
is selected based on the examinee's responses to the previous items.
Psychological tests are most reliable when item difficulties closely match
examinee ability.  CAT attempts to maximize reliability by selecting items
that are closest to examinees' estimated ability according to the items
already administered.  They statistical model used ensures that scores are
comparable across examinees, even though examinees take different items.

## 2. Where can I learn more about psychological measurement? ##

For a general introduction to Item Response Theory, see de Ayala's
[The Theory and Practice of Item Response Theory](http://www.goodreads.com/book/show/7738454-the-theory-and-practice-of-item-response-theory).
For an introduction to cognitive diagnosis (latent classification) and one of
the many available descrete models, see Jimmy de la Torre's
"[DINA Model and Parameter Estimation: A Didactic](http://dx.doi.org/10.3102/1076998607309474),"
_Journal of Educational and Behavioral Statistics_ 2009, v 34, p 115.  For
an introduction to concepts in CAT, see Howard Wainer's
[Computerized Adaptive Testing: A Primer](http://www.worldcat.org/oclc/634736628).

## 3. What is OSCATS and what can it do for me? ##

The Open Source Computerized Adaptive Testing System (OSCATS) is primarily
a library for psychometric models and algorithms used in CAT.  This library
can be used in conducting simulations for CAT research or as part of the
engine or back-end for an operational CAT.  OSCATS is designed to be
flexible and extensible, allowing for the easy addition of new models and
algorithms for rapid CAT development.  More components of the system (such
as an item parameter estimation suite, a simulation designer, and a test
administration front-end) are planned for future development.

## 4. Can I use OSCATS to give a test to my students? ##

Not currently.  For the time being, OSCATS is a library that could be
incorporated into a large CAT administration program, and is particularly
well suited for CAT simulation studies.

## 5. What's the GPL? ##

The [GNU General Public License](http://www.gnu.org/licenses/gpl.html)
(GPL) is a means for creating free (in the sense of liberty) software.  You
have the right to view and modify the source code of any software program
distributed to you under the GPL, and you are required to extend this right
to anyone to whom you also distribute any modifications.  This means that
you have complete freedom to use OSCATS in its entirety or chop it into
little bits and mangle it to pieces, but if you distribute any program that
makes use of OSCATS, you must make the complete source code of your program
(not just the OSCATS part) available as well.  Consequently, you are not
permitted to distribute programs that link OSCATS with proprietary or
closed-source components.

## 6. Does the GPL require that I release my items or item parameters? ##

No!  The GPL concerns the copyright of computer program source code, not
any data (such as items and item parameters) read into or written out from a
program covered by the GPL.

## 7. Do I have to release programs I make with OSCATS on the internet? ##

No.  You are not required to distribute any modifications you make to
OSCATS.  But, if you do distribute a program that uses OSCATS, you are
required to make the source code available on a comperable medium (e.g. if
you distribute the program on CD, you may send the source code on CD).  For
more information, see the GPL
[FAQ](http://www.gnu.org/licenses/gpl-faq.html#GPLRequireSourcePostedPublic).
Note that you are not allowed to distribute a program that makes use of
OSCATS under a
[nondisclosure agreement](http://www.gnu.org/licenses/gpl-faq.html#DoesTheGPLAllowNDA).

## 8. Can I sell programs I make with OSCATS? ##

Yes!  You may charge a fee when you distribute any program that makes use
of OSCATS, but you must make the source code available at no additional
cost.  For more information, see the GPL
[FAQ](http://www.gnu.org/licenses/gpl-faq.html#DoesTheGPLAllowMoney).

# Development #

## 1. In what sense is OSCATS extensible? ##

OSCATS features a modular design for item psychometric model and CAT
algorithms.  CAT algorithms connect themselves to one or more of eight
points in the test administration loop.  Adding a new model or algorithm
involves no more than writting code to perform the specific computations
desired.  Different models and algorithms can be exchanged with ease.

## 2. Why is OSCATS written in C as opposed to X high-level language? ##

The choice of the C programming language for OSCATS was perhaps more
strategic than practical.  A natively compiled language, like C, provides a
considerable speed advantage over easier-to-use but interpreted languages,
such as Python or Matlab.  Moreover, the
[GObject](http://library.gnome.org/devel/gobject/stable/) framework, written
in C, was designed to facilitate the creation of bindings for other
languages to object-oriented libraries written in C.  And, due to the
popularity of [GTK](http://www.gtk.org), which is also based on GObject,
there already exist systems for automatically generating GObject bindings
for a variety of popular languages.  In any case, it is not necessary to
know C in order to use and develop with OSCATS.  Since OSCATS comes with
language bindings to Perl, Python, PHP, Java, and Matlab, developers have
their choice of using (and writing extensions for) OSCATS in a number of
high-level languages.

## 3. Why didn't you include my favorite model/algorithm? ##

OSCATS started with a limited number of popular psychometric models and
CAT algorithms.  The exclusion of any particular model or algorithm was
merely due to the pragmatic need to code some things before others, and
shouldn't be considered a statement on the suitability of any model or
algorithm for a particular CAT application.  OSCATS is meant to grow over
time, and many more models and algorithms will be added as OSCATS grows.

## 4. Can you add my favorite model/algorithm? ##

Gladly!  Submit a
[feature request](http://code.google.com/p/oscats/issues/entry?template=Feature%20Request)
in the issue tracking system.  Better yet, code and [submit](Contributing.md) the
model or algorithm yourself.  (See the documentation for details on how to
extend OSCATS.)  You are encouraged to discuss potential inclusions on the
OSCATS mailing list.

## 5. What should I do if I'm having trouble using OSCATS? ##

Send a note to the [mailing list](http://groups.google.com/group/oscats)
(oscats@googlegroups.com) with a detailed description of the problem.

## 6. What should I do if I think I found an error in OSCATS? ##

Check to see if the error is a
[known issue](http://code.google.com/p/oscats/issues/list).  If not, please
file a report in the
[issue tracker](http://code.google.com/p/oscats/issues/entry?template=Defect%20report%20from%20user).
Be sure to include a detailed description of what you observed and what you
were expecting, including how to reproduce the behavior.

## 7. How can I help? ##

In many ways:
  * Use OSCATS in your CAT-related project.
  * Tell your friends about OSCATS.
  * Participate in scintillating discussion on the OSCATS mailing list.
  * Report errors or difficulties with OSCATS.
  * Write and submit code for new models and algorithms.
  * Tackle something on the [TODO](Features.md) list.
  * Write a wiz-bang test administration front-end that uses OSCATS.