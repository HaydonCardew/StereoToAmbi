files=$(find . -type f -name "*.png")
NewDir="RescaledAssets"
mkdir -p $NewDir
for file in $files
do
  ffmpeg -i $file -vf scale="iw/3:ih/3" $NewDir/$file
done
