#
# Copyright (c) 2012 Kimball Thurston
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
# OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
#
# Filename: ninja.py
#
# Author: Kimball Thurston
#
#

from . import ninja_syntax

class Ninja(ninja_syntax.Writer):
    """Wrapper class to abstract output"""
    def __init__( self ):
        ninja_syntax.Writer.__init__( self, None )
        return

    def open( self, filename ):
        self.output = open( filename, 'w' )
        return

    def close( self ):
        self.output.close()
        self.output = None
        return

    def write( self, rules, targets ):
        if self.output is None:
            sys.stdout.write( "Output object not opened for write" )
            sys.exit( 1 )
        for r in rules:
            r.write( self )
        for t in targets:
            t.write( self )
        return
    pass

class Rule:
    """A simple class representing a build rule"""
    name = None
    command = None
    description = None
    depfile = None
    generator = False
    restat = None
    rspfile = None
    rspfile_content = None

    def __init__( self, name, command, description=None, depfile=None,
                  generator=False, restat=False, rspfile=None, rspfile_content=None ):
        self.name = name
        self.command = command
        self.description = description
        self.depfile = depfile
        self.generator = generator
        self.restat = restat
        self.rspfile = rspfile
        self.rspfile_content = rspfile_content
        return

    def write( self, out ):
        out.rule( self.name, self.command,
                  description = self.description,
                  depfile = self.depfile,
                  generator = self.generator,
                  restat = self.restat,
                  rspfile = self.rspfile,
                  rspfile_content = self.rspfile_content )
        return
    pass

class Target:
    """Class representing a target"""
    def __init__( self ):
        return

    def write( self, out ):
        out.build( )
        return

    pass
