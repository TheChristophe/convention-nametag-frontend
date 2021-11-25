export function makeThumbnailUrl(file: File, setThumbnailUrl: (url: string) => void) {
    // Credits to Aske Ertmann
    // Sourced from: https://stackoverflow.com/a/39754219
    let fileReader = new FileReader();
    fileReader.onload = function () {
        let blob = new Blob([fileReader.result as BlobPart], {
            type: file.type,
        });
        let url = URL.createObjectURL(blob);
        let video = document.createElement('video');
        let timeupdate = function () {
            if (snapImage()) {
                video.removeEventListener('timeupdate', timeupdate);
                video.pause();
            }
        };
        video.addEventListener('loadeddata', function () {
            if (snapImage()) {
                video.removeEventListener('timeupdate', timeupdate);
            }
        });
        let snapImage = function () {
            let canvas = document.createElement('canvas');
            canvas.width = video.videoWidth;
            canvas.height = video.videoHeight;
            canvas.getContext('2d')!.drawImage(video, 0, 0, canvas.width, canvas.height);
            let image = canvas.toDataURL();
            let success = image.length > 100000;
            if (success) {
                setThumbnailUrl(image);
                console.log(image);
                URL.revokeObjectURL(url);
            }
            return success;
        };
        video.addEventListener('timeupdate', timeupdate);
        video.preload = 'metadata';
        video.src = url;
        // Load video in Safari / IE11
        video.muted = true;
        video.playsInline = true;
        video.play();
    };
    fileReader.readAsArrayBuffer(file);
}
