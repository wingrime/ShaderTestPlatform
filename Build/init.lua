function init(dt)
	env = Env.new()
	--env:loge("message from lua _loge");
	--env:logw("message from lua _logw");
	--env:setUpdateCallFrequency(1);

	time = 0.0;
end

function update(dt)
	
	time = time + dt;
	env:loge("dt ".. dt);
	env:fwd(30.0*math.sin(time/100.0));
end