import { createClient } from "redis";
import { configuration } from "../config";


export class redisAdaptersPubsub {


    constructor(iq: any) {
        let redisConfig: any = {
            host: configuration.redis_client.host,
            port: configuration.redis_client.port,
        };
        console.log("redisConfig >>>>.", redisConfig);
        if (process.env.NODE_ENV === "staging") {
            redisConfig.no_ready_check = configuration.redis_client.no_ready_check;
            redisConfig.auth_pass = configuration.redis_client.auth_pass;
        }

        const url = `redis://${configuration.redis_client.host}:${configuration.redis_client.port}`

        const redisClient = createClient({ url, password: configuration.redis_client.auth_pass });




        const subscriber = redisClient.duplicate();


        /*     subscriber.connect().then((result:any) => {
        
        console.log('Redis server connected');
        
        // subscriber.connect();
        
        }).catch(()=> {
        
        console.log('Redis server not connected');
        
        }); */



    }


}
