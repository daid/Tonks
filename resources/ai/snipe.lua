function update()
    local angle = (getPlayer(0).getPosition() - self.getPosition()):angle()
    local diff = angle-self.getTurretRotation()
    if diff > 180 then diff = diff - 360 end
    if diff < -180 then diff = diff + 360 end
    if diff > 5 then
        self.rotate(1)
    elseif diff < -5 then
        self.rotate(-1)
    else
        self.fire()
        for n=0,60 do
            yield()
        end
    end
end
