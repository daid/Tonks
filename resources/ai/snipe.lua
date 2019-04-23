function getAngleDiffTowardsPlayer(index)
    local player = getPlayer(index)
    if player == nil or not player.valid then
        return nil, nil
    end
    local angle = (player.getPosition() - self.getPosition()):angle()
    local diff = angle-self.getTurretRotation()
    if diff > 180 then diff = diff - 360 end
    if diff < -180 then diff = diff + 360 end
    local score = math.abs(diff) + (player.getPosition() - self.getPosition()):length() * 5.0
    return diff, score
end

function update()
    local diff0, score0 = getAngleDiffTowardsPlayer(0)
    local diff1, score1 = getAngleDiffTowardsPlayer(1)
    local diff = diff0
    print(score0, score1)
    if diff == nil or score0 > score1 then diff = diff1 end
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
