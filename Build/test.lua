--  test script
function init(dt)

	env = Env.new()


	env:loge("message from lua _loge");
	env:logw("message from lua _logw");
	
	time = 0.0;
end

function update(dt)
	time += 0.1;
	-- todo
end