import  os
def main():
    BASEDIR = "D:/codeprograms/mybuilds/vtk9.0install/lib"

    fn = os.listdir(BASEDIR)
    fn = list(filter(lambda x: os.path.isfile(BASEDIR+"/"+x), fn))
    with open("lib.txt",'w') as fp:
        for i in fn:
            fp.write(f'LIBS += {BASEDIR}/{i}\n')
    print(f"Libs generated for {BASEDIR}")

if __name__ == "__main__":
	main()
