function angleDiff(a, b)
    local diff = a - b
    if diff > 180 then diff = diff - 360 end
    if diff < -180 then diff = diff + 360 end
    return diff
end

function getTarget()
    local p0 = getPlayer(0)
    local p1 = getPlayer(1)
    if p0 == nil then return p1 end
    if p1 == nil then return p0 end
    
    if (self.getPosition() - p0.getPosition()):length() < (self.getPosition() - p1.getPosition()):length() then
        return p0
    end
    return p1
end

function update()
    local target = getTarget()
    if target == nil then return end
    
    local diff = target.getPosition() - self.getPosition()
    local distance = diff:length()
    if distance > 3.5 then
        self.move(diff)
    elseif distance < 3.0 then
        self.move(-diff)
    end
    
    local turret_diff = angleDiff(diff:angle(), self.getTurretRotation())
    self.rotate(turret_diff * 0.1) -- Aim at the target
    if math.abs(turret_diff) < 1 and distance < 4.0 then
        self.fire()
        for n=0,60 do
            yield()
        end
    end
end
