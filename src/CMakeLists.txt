target_sources(${MY_PROYECT}
	PRIVATE
		main/main.cpp
		main/scanner/ScannerFile.cpp
)

if(TEST)

	target_sources(${MY_PROYECT}
		PRIVATE
			main/main.cpp
	)

else(TEST)
    
	target_sources(${MY_PROYECT}
		PRIVATE
			test/main.cpp
	)

endif(TEST)