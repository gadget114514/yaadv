class Ns {
	foreign static say_(a,b,c,d)
	static say(a,b,c,d) {
		say_(a,b,c,d)
		Fiber.suspend()
	}
	foreign static leave(a)
	foreign static fg(a)
	foreign static bg(a,b)
	foreign static jump_(a)
	static jump(a){
		jump_(a)
	}
	foreign static bgm(a)
	foreign static stopbgm()
	foreign static sound(a)
	foreign static stopsound()
	foreign static gameover()
	foreign static set(a,b,c)
	foreign static select(a)
	foreign static selectoption(a,b)
	foreign static selectshow_(a)
	static selectshow(a) {
		selectshow_(a,b,c)
		Fiber.suspend()
	}
	foreign static clear(a)
	foreign static print(a)
	foreign static tween(a,b,c,d)
	foreign static img(a,b,c) 
	foreign static leaveimg(a) 			
	foreign static saveFiber(a)
	foreign static s_(cmd,a)
	foreign static ss_(cmd,a,b)
	foreign static sss_(cmd,a,b,c)
	foreign static v_(cmd)
}
