import { useMutation } from '@tanstack/react-query';
import { HOST } from './config';
import type VideoMetadata from 'components/VideoMetadata';

type ThumbnailProps = { thumbnailUrl: string; className?: string };
export const Thumbnail = ({ thumbnailUrl, className }: ThumbnailProps) => (
    <div
        style={{
            height: '100%',
            backgroundImage: thumbnailUrl.length ? 'url(' + thumbnailUrl + ')' : undefined,
            backgroundSize: 'cover',
        }}
        className={className}
    />
);

const humanReadableTime = (seconds: number) => {
    const minutes = Math.floor(seconds / 60);
    const remainingSeconds = Math.floor(seconds % 60);
    return minutes.toString().padStart(2, '0') + ':' + remainingSeconds.toString().padStart(2, '0');
};

type VideoEntryProps = { metadata: VideoMetadata; reload: () => void };
const VideoEntry = ({ metadata, reload }: VideoEntryProps) => {
    const play = useMutation({
        mutationFn: () =>
            fetch(HOST + '/videos/' + metadata.filename + '/play', {
                method: 'POST',
            }),
    });

    const delete_ = useMutation({
        mutationFn: () =>
            fetch(HOST + '/videos/' + metadata.filename, {
                method: 'DELETE',
            }),
        onSuccess: () => {
            reload();
        },
    });

    const thumbnailUrl = metadata.thumbnail
        ? `${HOST}/thumbnails/${metadata.thumbnail}`
        : 'https://i.imgur.com/C3QGaPB.jpeg';

    return (
        <div className="grow flex flex-row border rounded p-1">
            <button onClick={() => play.mutate()} className="flex flex-row grow">
                <Thumbnail thumbnailUrl={thumbnailUrl} className="flex-grow" />
                <div className="p-1">
                    {metadata.filename}
                    <br />({metadata.duration ? humanReadableTime(metadata.duration) : '??:??'})
                </div>
            </button>
            <button
                onClick={() => delete_.mutate()}
                aria-label="Delete"
                className="block border-l-2 p-1"
            >
                🗑️
            </button>
        </div>
    );
};

export default VideoEntry;
