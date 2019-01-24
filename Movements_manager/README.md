This manager program will call other programs to process the videos.

First step is to generate files of coordinates from video files 
where each coordinate might be an object in the video, we call this blobs.

The next step is to analyze the blobs. Group multiple blobs to one object. We call this tracks.
Each track is a list of blobs and can be an object. The best result is if each object produce only one track.

The next step is to analyze the tracks. We assume each track comes from one object.
Each track can then be classifed as (boat, human, airplane, bird).
