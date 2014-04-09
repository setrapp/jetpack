all:
	gcc MLModelViewer.c -o MLModelViewer.o -L../TextureLoad -L../GeneralUtility -ltl -lgu -lGL -c
	gcc MLMaterialLoader.c -o MLMaterialLoader.o -L../TextureLoad -L../GeneralUtility -ltl -lgu -lGL -c
	#gcc -static-libgcc MLModelViewer.c -L../TextureLoad -L../GeneralUtility -ltl -lgu -lopengl32 -o MLModelViewer.o -c
	#gcc -static-libgcc MLMaterialLoader.c -L../TextureLoad -L../GeneralUtility -ltl -lgu -lopengl32 -o MLMaterialLoader.o -c
	ar -r libml.a MLModelViewer.o
	ar -r libml.a MLMaterialLoader.o
