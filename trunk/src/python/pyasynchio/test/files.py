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