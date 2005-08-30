# Copyright (c) 2005 Vladimir Sukhoy

# Permission is hereby granted, free of charge, to any person obtaining a copy 
# of this software and associated documentation files (the "Software"), to deal 
# in the Software without restriction, including without limitation the rights 
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
# copies of the Software, and to permit persons to whom the Software is furnished 
# to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all 
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
# PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


import pyasynchio
import unittest
import os

class TestFiles(unittest.TestCase):
    def setUp(self):
        self.filename = 'test_file.bin'
        self.file = file(self.filename, 'w+')
        self.apoll = pyasynchio.apoll()
    
    def tearDown(self):
        del self.apoll
        self.file.close()
        os.unlink(self.filename)

    def test_writing(self):
        text = 'A long time ago in a galaxy far away.' * 50
        pieces = 13
        for piece in xrange(pieces):
            offset = (len(text) * piece) // 13
            next = (len(text) * (piece + 1)) // 13
            self.apoll.write(self.file, offset, text[offset:next])
        
        done = 0
        while done < pieces:
            events = self.apoll.poll()
            for evt in events:
                if evt['type'] == 'write' and evt['success'] == True:
                    done += 1

        self.file.close()
        self.file = file(self.filename, 'r')
        self.assert_(self.file.read() == text)

    def test_reading(self):
        text = 'Join the dark side :).' * 50
        self.file.write(text)
        self.file.close()
        self.file = file(self.filename, 'r')
        pieces = 13
        for piece in xrange(pieces):
            offset = (len(text) * piece) // 13
            next = (len(text) * (piece + 1)) // 13
            self.apoll.read(self.file, offset, next - offset)

        done = 0
        text2 =  []
        while done < pieces:
            events = self.apoll.poll()
            for evt in events:
                if evt['type'] == 'read' and evt['success'] == True:
                    self.assert_(evt['data'] 
                        == text[evt['offset']:evt['offset'] + len(evt['data'])])
                    text2.append((evt['offset'], evt['data']))
                    done += 1
        text2.sort()
        textt = ''
        for t in text2:
            textt += t[1]
        self.assert_(textt == text)
        

suite = unittest.TestSuite()
suite.addTest(unittest.makeSuite(TestFiles))