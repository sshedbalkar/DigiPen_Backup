function Body( object )
	testfn2 = function( msg )
		--if( msg.MessageId == Message.MouseMove ) then
			--print(msg.MessageId);
		--end
	end
	testfn =  function()
		print("\nLuaTest_1: ");
		--print( tostring(object) );
		print("\n");
		--print1(45);
		local v = CTestClass1();
		v:TestFn1(1, 23.45);
		ObjTest:TestFn2("omg its working");
		object:TestFn2("omg this is also working");
		--
		AddMsgSubscriber( testfn2 );
		coroutine.yield();
	end
	testfn();
end