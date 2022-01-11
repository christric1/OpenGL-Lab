from Line import Line

pi = 3.14159265
frame_count = 10


class LinePair:
    def __init__(self):
        self.leftLine = Line()
        self.rightLine = Line()
        self.warpLine = []

    def genWarpLine(self):
        while self.leftLine.degree - self.rightLine.degree > pi:
            self.rightLine.degree = self.rightLine.degree + pi
        while self.rightLine.degree - self.leftLine.degree > pi:
            self.leftLine.degree = self.leftLine.degree + pi

        for i in range(frame_count):
            ratio = (i + 1) / (frame_count + 1)
            curLine = Line()

            curLine.M.x = (1 - ratio) * self.leftLine.M.x + ratio * self.rightLine.M.x
            curLine.M.y = (1 - ratio) * self.leftLine.M.y + ratio * self.rightLine.M.y
            curLine.len = (1 - ratio) * self.leftLine.len + ratio * self.rightLine.len
            curLine.degree = (1 - ratio) * self.leftLine.degree + ratio * self.rightLine.degree

            curLine.MLDtoPQ()
            self.warpLine.append(curLine)
