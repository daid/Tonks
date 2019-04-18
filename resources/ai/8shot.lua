function update()
    for n=1,8 do
        yield()
    end
    self.rotate(45)
    for n=1,30 do
        yield()
    end
    self.fire()
end