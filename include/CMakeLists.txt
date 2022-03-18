target_sources(${MY_PROYECT}
	PRIVATE
		scanner/ScannerFile.hpp
	PUBLIC
		# anomaly_detection/IAnomalyDetector.hpp
		# object_characterization/IObjectCharacterizator.hpp
		scanner/IScanner.hpp
		CharacteristicsVector.hpp
		Model.hpp
		ModelManager.hpp
		Point.hpp

)