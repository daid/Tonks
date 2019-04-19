function getAngleDiffTowardsPlayer(index)
    local player = getPlayer(index)
    if player == nil or not player.valid then
        return nil
    end
    local angle = (player.getPosition() - self.getPosition()):angle()
    local diff = angle-self.getTurretRotation()
    if diff > 180 then diff = diff - 360 end
    if diff < -180 then diff = diff + 360 end
    return diff
end

function update()
    local diff0 = getAngleDiffTowardsPlayer(0)
    local diff1 = getAngleDiffTowardsPlayer(1)
    local diff = diff0
    if diff == nil or math.abs(diff) > math.abs(diff1) then diff = diff1 end
    if diff == nil then return end
    
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
