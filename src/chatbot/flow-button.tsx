


import React from 'react';

const ButtonComponent = ({data,handleTempInput}:any) => {
    let filtered_array = data?.message?.buttonDataArr?.children?.filter((o:any) => o.type !== "success" && o.type !== "default" && o.type !== "error" && o.type !== "validate");
    const unique = [];
   filtered_array && filtered_array.map((x:any) => unique.filter(a => a.type == x.type).length > 0 ? null : unique.push(x));
    const linkData = data?.message?.buttonDataArr?.information?.data !== undefined ? JSON.parse(data?.message?.buttonDataArr?.information?.data) : {};
    const updatedArr1 = unique.map(item1 => {
        const matchingItem = linkData?.getInputData.find((item2:any) => item2.value === item1.type);
        if (matchingItem) {
          return { ...item1, link: matchingItem.link, color: matchingItem.color || '' };
        }
        return item1;
    });
    return(
        <div>
            <div className="messagedialogBtn">
            {
                updatedArr1?.map((item, index) => {
                return (
                    <>
                    {item?.type !== undefined && <div
                        style={item?.color ? {color: item?.color,border: `1px solid ${item?.color}`, background:`${item?.color}0f` } : {}}
                        className="dialogBtn green"
                        onClick={() => {
                            handleTempInput(item, data?.message,data?.message?.buttonDataArr?.children)
                        }}
                    >
                        {item.type}
                    </div>}
                    </>
                );
                })
            }
            </div>
        </div>
    )
}

export default React.memo(ButtonComponent);